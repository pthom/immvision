# Install pip package "immvision"

## Create a virtual environment at the root of the repository

    cd immvision/
    python3 -m venv venv
    source venv/bin/activate


## Normal install

    unset IMMVISION_PIP_EDITABLE
    pip install -v .
    python3 -c 'import immvision'

## Editable install

    export IMMVISION_PIP_EDITABLE=1
    pip install -v -e .
    python3 -c 'import immvision'

