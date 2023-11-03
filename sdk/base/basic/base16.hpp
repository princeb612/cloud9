/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *  RFC 4648 The Base16, Base32, and Base64 Data Encodings
 *
 * Revision History
 * Date         Name                Description
 *
 * @example
 *          const char* message = "sample";
 *          std::string hex;
 *          binary_t bin;
 *          base16_encode (message, 6, hex);
 *          std::cout << hex.c_str () << std::endl;
 *          base16_decode (hex, bin);
 *          basic_stream bs;
 *          dump_memory (&bin[0], bin.size (), &bs);
 *          printf ("%s\n", bs.c_str ());
 *
 *          const char* encoded1 = "01020304";
 *          bin1 = base16_decode (encoded1);
 *          const char* encoded2 = "0x01020304";
 *          bin2 = base16_decode (encoded2);
 */

#ifndef __HOTPLACE_SDK_BASE_BASIC_BASE16__
#define __HOTPLACE_SDK_BASE_BASIC_BASE16__

#include <sdk/base/error.hpp>
#include <sdk/base/stream.hpp>

namespace hotplace {

/*
 * @brief   encode
 * @param   const byte_t* source [in]
 * @param   size_t size [in]
 * @param   char* buf [out]
 * @param   size_t* buflen [out]
 * @example
 *          binary_t source = convert ("wild wild world");
 *          size_t size = 0;
 *          base16_decode (&source[0], source.size, nullptr, &size);
 *          char* buf = (char*) malloc (size);
 *          base16_decode (&source[0], source.size, buf, &size);
 *          free (buf);
 */
return_t base16_encode(const byte_t* source, size_t size, char* buf, size_t* buflen);
/*
 * @brief   encode
 * @param   const byte_t* source [in]
 * @param   size_t size [in]
 * @param   std::string& outpart [out]
 * @example
 *          binary_t source = convert ("wild wild world");
 *          std::string encoded;
 *          base16_decode (&source[0], source.size, encoded);
 */
return_t base16_encode(const byte_t* source, size_t size, std::string& outpart);
/*
 * @brief   encode
 * @param   const byte_t* source [in]
 * @param   size_t size [in]
 * @param   stream_t* stream [out]
 * @example
 *          binary_t source = convert ("wild wild world");
 *          basic_stream encoded;
 *          base16_decode (&source[0], source.size, &encoded);
 */
return_t base16_encode(const byte_t* source, size_t size, stream_t* stream);
/*
 * @brief   encode
 * @param   binary_t const& source [in]
 * @param   char* buf [out]
 * @param   size_t* buflen [out]
 * @example
 *          binary_t source = convert ("wild wild world");
 *          size_t size = 0;
 *          base16_decode (source, nullptr, &size);
 *          char* buf = (char*) malloc (size);
 *          base16_decode (source, buf, &size);
 *          free (buf);
 */
return_t base16_encode(binary_t const& source, char* buf, size_t* buflen);
/*
 * @brief   encode
 * @param   binary_t const& source [in]
 * @param   std::string& outpart [out]
 * @example
 *          binary_t source = convert ("wild wild world");
 *          std::string encoded;
 *          base16_decode (source, encoded);
 */
return_t base16_encode(binary_t const& source, std::string& outpart);
/*
 * @brief   encode
 * @param   binary_t const& source [in]
 * @param   stream_t* stream [out]
 * @example
 *          binary_t source = convert ("wild wild world");
 *          basic_stream encoded;
 *          base16_decode (source, &encoded);
 */
return_t base16_encode(binary_t const& source, stream_t* stream);
/*
 * @brief   encode
 * @param   binary_t const& source [in]
 * @example
 *          binary_t source = convert ("wild wild world");
 *          std::string encoded = base16_decode (source);;
 */
std::string base16_encode(binary_t const& source);

std::string base16_encode(const char* source);
return_t base16_encode(const char* source, std::string& outpart);
return_t base16_encode(const char* source, binary_t& outpart);
return_t base16_encode(std::string const& source, binary_t& outpart);

/**
 * @brief   decode
 * @param   const char* source [in]
 * @param   size_t size [in]
 * @param   binary_t& outpart [out]
 */
return_t base16_decode(const char* source, size_t size, binary_t& outpart);
/**
 * @brief   decode
 * @param   const char* source [in]
 * @param   size_t size [in]
 * @param   stream_t* stream [out]
 */
return_t base16_decode(const char* source, size_t size, stream_t* stream);
/**
 * @brief   decode
 * @param   std::string const& source [in]
 * @param   binary_t& outpart [out]
 */
return_t base16_decode(std::string const& source, binary_t& outpart);
/**
 * @brief   decode
 * @param   std::string const& source [in]
 * @param   stream_t* stream [out]
 */
return_t base16_decode(std::string const& source, stream_t* stream);
/**
 * @brief   decode
 * @param   const char* source [in]
 * @return  binary_t
 */
binary_t base16_decode(const char* source);
/**
 * @brief   decode
 * @param   const char* source [in]
 * @param   size_t size [in]
 * @return  binary_t
 */
binary_t base16_decode(const char* source, size_t size);
/**
 * @brief   decode
 * @param   std::string const& source [in]
 * @return  binary_t
 */
binary_t base16_decode(std::string const& source);

/**
 * @brief encode
 * @example
 *      // RFC 7516
 *      // Initialization Vector [227, 197, 117, 252, 2, 219, 233, 68, 180, 225, 77, 219]
 *      std::string iv = base16_encode_rfc("[227, 197, 117, 252, 2, 219, 233, 68, 180, 225, 77, 219]");
 *      // RFC 7539
 *      // Key = 00:01:02:03:04:05:06:07:08:09:0a:0b:0c:0d:0e:0f:10:11:12:13:14:15:16:17:18:19:1a:1b:1c:1d:1e:1f
 *      std::string key = base16_encode_rfc("00:01:02:03:04:05:06:07:08:09:0a:0b:0c:0d:0e:0f:10:11:12:13:14:15:16:17:18:19:1a:1b:1c:1d:1e:1f");
 *      //  000  80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f  ................
 *      //  016  90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f  ................
 *      binary_t key = base16_encode_rfc("80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
 *                                       "90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f");
 */
std::string base16_encode_rfc(std::string const& source);
binary_t base16_decode_rfc(std::string const& source);

}  // namespace hotplace

#endif
