@echo ff
echo installing Twine to use it to publish the wheel files.
python -m pip install twine
echo publishing the wheel files with twine (you'll need a 2FA token from your phone).
python -m twine upload wheelhouse/*.whl