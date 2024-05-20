{
    'targets': [
        {
            'target_name': 'ibft',
            'sources': [ 'src/binding.cpp' ],
            'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
            'cflags_cc!': ['-fno-exceptions'] 
        }
    ]
}
