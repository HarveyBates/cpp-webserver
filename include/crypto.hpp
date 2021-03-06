/* Modified from https://gitlab.com/eidheim/Simple-WebSocket-Server
* Original author: Ole Christian Eidheim
* Modified by: Harvey Bates
* Purpose: Easy way to decode, encode and decrypt Websocket messages
*/

#ifndef SIMPLE_WEB_CRYPTO_HPP
#define SIMPLE_WEB_CRYPTO_HPP

#pragma once

#include <cmath>
#include <iomanip>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

class Crypto {
	const static std::size_t buffer_size = 131072;

	public:
	class Base64 {
		public:
			static std::string encode(const std::string &input) noexcept {
				std::string base64;

				BIO *bio, *b64;
				BUF_MEM *bptr = BUF_MEM_new();

				b64 = BIO_new(BIO_f_base64());
				BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
				bio = BIO_new(BIO_s_mem());
				BIO_push(b64, bio);
				BIO_set_mem_buf(b64, bptr, BIO_CLOSE);

				// Write directly to base64-buffer to avoid copy
				auto base64_length = static_cast<std::size_t>(round(4 * ceil(static_cast<double>(input.size()) / 3.0)));
				base64.resize(base64_length);
				bptr->length = 0;
				bptr->max = base64_length + 1;
				bptr->data = &base64[0];

				if(BIO_write(b64, &input[0], static_cast<int>(input.size())) <= 0 || BIO_flush(b64) <= 0)
					base64.clear();

				// To keep &base64[0] through BIO_free_all(b64)
				bptr->length = 0;
				bptr->max = 0;
				bptr->data = nullptr;

				BIO_free_all(b64);

				return base64;
			}

			static std::string decode(const std::string &base64) noexcept {
				std::string ascii;

				// Resize ascii, however, the size is a up to two bytes too large.
				ascii.resize((6 * base64.size()) / 8);
				BIO *b64, *bio;

				b64 = BIO_new(BIO_f_base64());
				BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
				// TODO: Remove in 2020
#if OPENSSL_VERSION_NUMBER <= 0x1000115fL
				bio = BIO_new_mem_buf(const_cast<char *>(&base64[0]), static_cast<int>(base64.size()));
#else
				bio = BIO_new_mem_buf(&base64[0], static_cast<int>(base64.size()));
#endif
				bio = BIO_push(b64, bio);

				auto decoded_length = BIO_read(bio, &ascii[0], static_cast<int>(ascii.size()));
				if(decoded_length > 0)
					ascii.resize(static_cast<std::size_t>(decoded_length));
				else
					ascii.clear();

				BIO_free_all(b64);

				return ascii;
			}
	};

	/// Return hex string from bytes in input string.
	static std::string to_hex_string(const std::string &input) noexcept {
		std::stringstream hex_stream;
		hex_stream << std::hex << std::internal << std::setfill('0');
		for(auto &byte : input)
			hex_stream << std::setw(2) << static_cast<int>(static_cast<unsigned char>(byte));
		return hex_stream.str();
	}


	static std::string sha1(const std::string &input, std::size_t iterations = 1) noexcept {
		std::string hash;

		hash.resize(160 / 8);
		SHA1(reinterpret_cast<const unsigned char *>(&input[0]), input.size(), reinterpret_cast<unsigned char *>(&hash[0]));

		for(std::size_t c = 1; c < iterations; ++c)
			SHA1(reinterpret_cast<const unsigned char *>(&hash[0]), hash.size(), reinterpret_cast<unsigned char *>(&hash[0]));

		return hash;
	}

	static std::string sha1(std::istream &stream, std::size_t iterations = 1) noexcept {
		SHA_CTX context;
		SHA1_Init(&context);
		std::streamsize read_length;
		std::vector<char> buffer(buffer_size);
		while((read_length = stream.read(&buffer[0], buffer_size).gcount()) > 0)
			SHA1_Update(&context, buffer.data(), static_cast<std::size_t>(read_length));
		std::string hash;
		hash.resize(160 / 8);
		SHA1_Final(reinterpret_cast<unsigned char *>(&hash[0]), &context);

		for(std::size_t c = 1; c < iterations; ++c)
			SHA1(reinterpret_cast<const unsigned char *>(&hash[0]), hash.size(), reinterpret_cast<unsigned char *>(&hash[0]));

		return hash;
	}

};
#endif /* SIMPLE_WEB_CRYPTO_HPP */
