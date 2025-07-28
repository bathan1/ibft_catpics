{
    "targets": [
        {
            "target_name": "ibft",
            "sources": [ "src/binding.cpp", "src/blockchain.cpp", "src/crypto_utils.cpp", "src/transaction.cpp", "src/node.cpp", "src/message.cpp" ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            "cflags_cc!": ["-fno-exceptions"],
            "libraries": []
        }
    ]
}
