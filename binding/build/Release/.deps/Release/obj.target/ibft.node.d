cmd_Release/obj.target/ibft.node := g++ -o Release/obj.target/ibft.node -shared -pthread -rdynamic -m64  -Wl,-soname=ibft.node -Wl,--start-group Release/obj.target/ibft/src/binding.o Release/obj.target/ibft/src/blockchain.o Release/obj.target/ibft/src/crypto_utils.o Release/obj.target/ibft/src/transaction.o Release/obj.target/ibft/src/node.o Release/obj.target/ibft/src/message.o -Wl,--end-group 