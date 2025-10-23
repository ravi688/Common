#!/bin/bash
# Usage: ./run_build.sh <build_dir>

# Exit immediately if no argument is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <build_dir>"
    exit 1
fi

BUILD_DIR="$1"

# Check if the directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Directory '$BUILD_DIR' does not exist."
    exit 1
fi

# Run the test binary
if ! "./${BUILD_DIR}/main_test"; then
    echo "test failed"
    exit 1
fi

# Run the main binary
if ! "./${BUILD_DIR}/main"; then
    echo "test failed"
    exit 1
fi

echo "All tests passed successfully."

