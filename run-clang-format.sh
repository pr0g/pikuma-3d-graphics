#!/bin/bash

fd -0 -e h -e cpp -a -E "third-party/" -E "build/" | xargs -0 clang-format -i
