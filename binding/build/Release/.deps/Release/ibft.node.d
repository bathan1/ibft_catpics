cmd_Release/ibft.node := ln -f "Release/obj.target/ibft.node" "Release/ibft.node" 2>/dev/null || (rm -rf "Release/ibft.node" && cp -af "Release/obj.target/ibft.node" "Release/ibft.node")
