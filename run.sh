OUTPUT_FILE="main"

g++ -std=c++11 -o $OUTPUT_FILE src/*.cpp -Iinclude

if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

echo "Compilation successful\n"

./$OUTPUT_FILE

rm $OUTPUT_FILE

echo "Done"