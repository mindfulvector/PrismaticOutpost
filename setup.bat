scoop install jq
scoop install conan
conan profile detect
conan install . -s build_type=Release --build=missing
