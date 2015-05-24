# This script cleans up the old test coverage and re-runs it.

# Clean coverage!
make clean_cov
# Compile the coverage version again.
make CFG=coverage
# Run the tests of the coverage build.
valgrind bin/coverage/test
# Create the HTML coverage site.
make gcov
# Open it!
xdg-open cov/index.html
