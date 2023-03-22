# Documentation

This document describes how to generate documentation of the library from the sources.
If you're looking for the documentation please [follow this link](#).

## Prerequsites

1. Doxygen
2. Python 3.9+
3. Pipenv

## Installation

The first time setup is required before generating the documentation. Follow these steps:
1. Open terminal in the `docsrc` directory
2. Run `pipenv install`

### Apple Silicon M1

On Apple M1 processors you may get this error when trying to build the documentation: `Could not parse the contents of index.xml as an xml.`

Most likely this issue is caused by an architecture mismatch of the `lxml` package.
To fix this issue open the Pipenv shell by running `pipenv shell` and follow these steps:
1. Run `pip uninstall lxml`
2. Run `arch -arm64 pip install lxml --no-binary lxml`

This solution forces `lxml` to be built from the sources.

## Generating the documentation

1. Open terminal in the `docsrc` directory
2. Run the Doxygen command `doxygen`
3. Open the Pipenv shell `pipenv shell`
4. Run the build `make html`

The generated documentation will be available in the `docsrc/build/html` directory.

## Github pages

In order to build the documentation that's going to be published as Github pages run `make github`.
The generated HTML build will be automatically moved to the `docs` directory.
