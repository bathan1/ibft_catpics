cmake_minimum_required(VERSION 3.10)
project(ibft_demo)

find_package(OpenSSL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/blockchain.cpp src/transaction.cpp src/crypto_utils.cpp src/message.cpp)
target_link_libraries(${PROJECT_NAME} openssl::openssl)
