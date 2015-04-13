//
// multiget2.cpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2009-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <urdl/read_stream.hpp>
#include <asio/spawn.hpp>
#include <iostream>
#include <functional>
#include <fstream>
#include <string>

void download(asio::io_service& io_service,
    const urdl::url& url, const std::string& file,
    asio::yield_context yield)
{
  try
  {
    urdl::read_stream read_stream(io_service);
    read_stream.async_open(url, yield);

    std::ofstream ofstream(file.c_str(),
        std::ios_base::out | std::ios_base::binary);

    char buffer[1024];
    std::size_t length;
    asio::error_code ec;

    do
    {
      length = read_stream.async_read_some(
          asio::buffer(buffer), yield[ec]);
      ofstream.write(buffer, length);
    } while (length > 0);
  }
  catch (std::exception& e)
  {
    std::cerr << "Download exception: " << e.what() << std::endl;
  }
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 3 || argc % 2 == 0)
    {
      std::cerr << "Usage: multiget2 <url> <outputfile> ";
      std::cerr << "[<url> <outputfile> ...]\n";
      return 1;
    }

    asio::io_service io_service;

    for (int i = 1; i < argc; i += 2)
    {
      asio::spawn(io_service,
          std::bind(download, std::ref(io_service),
            urdl::url(argv[i]), std::string(argv[i + 1]), _1));
    }

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}
