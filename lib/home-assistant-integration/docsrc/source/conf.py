# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Project information -----------------------------------------------------

project = 'ArduinoHA'
copyright = '2022, Dawid Chyrzynski'
author = 'Dawid Chyrzynski'

# The full version, including alpha/beta/rc tags
release = '2.0.0'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'breathe',
    'sphinx.ext.extlinks',
    'sphinxcontrib.email'
]

# Add any paths that contain templates here, relative to this directory.
# templates_path = ['templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

# Setup the breathe extension
breathe_projects = {
    'ArduinoHA': '../xml'
}
breathe_default_project = 'ArduinoHA'
breathe_show_include = True
breathe_default_members = ('members', 'undoc-members')

# Tell sphinx what the primary language being documented is.
primary_domain = 'cpp'

# Tell sphinx what the pygments highlight language should be.
highlight_language = 'cpp'

extlinks = {
    'example': ('https://github.com/dawidchyrzynski/arduino-home-assistant/tree/main/examples/', None)
}

# -- Options for HTML output -------------------------------------------------

html_scaled_image_link = False
html_copy_source = False
html_theme = 'press-ext'
html_theme_path = ['themes']
html_title = 'ArduinoHA'
html_sidebars = {'**': ['util/searchbox.html', 'sidetoc.html']}
html_theme_options = {
    'external_links': [
        ('Github', 'https://github.com/dawidchyrzynski/arduino-home-assistant')
    ]
}
html_js_files = [
    'custom.js'
]
html_css_files = [
    'custom.css'
]

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named 'default.css' will overwrite the builtin 'default.css'.
html_static_path = ['static']

# -- Extension ---------------------------------------------------------------

email_automode = False
