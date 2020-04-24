
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
    url="https://github.com/",
    author="Evan Widloski",
    author_email="evan@evanw.org",
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: Implementation :: PyPy",
    ],
    # packages=find_packages(),
    packages=find_packages(),
    package_data={'':[
        'rapidalarm/algorithm.cdef',
        'firmware/algorithm.c',
        'firmware/algorithm.h'
    ]},
    include_package_data=True,
    # py_modules=['rapidalarm.rapidalarm', 'rapidalarm.algorithm'],
    entry_points={
        'console_scripts': ['rapidalarm = rapidalarm.rapidalarm:main']
    },
    setup_requires=["cffi>=1.0.0"],
    cffi_modules=[
        "./rapidalarm/build_algorithm.py:ffi",
    ],
    install_requires=[
        "cffi>=1.0.0",
        "matplotlib",
        "pandas",
        "numpy"
    ],
)
