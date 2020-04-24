
#!/usr/bin/env python

import os
import sys

from setuptools import setup, find_packages

# os.chdir(os.path.dirname(sys.argv[0]) or ".")

setup(
    name="rapidalarm",
    version="0.1",
    description="RapidAlarm simulation tools",
    long_description=open("README.md", "rt").read(),
    url="https://github.com/wolever/python-cffi-example",
    author="Evan Widloski",
    author_email="evan@evanw.org",
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: Implementation :: PyPy",
    ],
    packages=find_packages(),
    install_requires=["cffi>=1.0.0"],
    entry_points={
        'console_scripts': ['rapidalarm = python.rapidalarm:main']
    },
    setup_requires=["cffi>=1.0.0"],
    cffi_modules=[
        "./python/build_algorithm.py:ffi",
    ],
)
