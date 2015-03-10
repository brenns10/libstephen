# Build GitHub Pages branch automatically.

## STARTING STATE: Must be in the branch master, with no uncommitted changes.
## You've run the tests on release mode, and they run successfully!

# Get the current commit
COMMIT=$(git rev-parse HEAD)

# Generate coverage information.
make CFG=coverage
bin/coverage/test
make gcov

# Generate documentation.
make docs

# Switch to the gh-pages branch, remove the build artifacts.
git checkout gh-pages
rm -rf bin obj
git commit -m "[ci skip] Autodoc commit for $COMMIT."
git push origin gh-pages
