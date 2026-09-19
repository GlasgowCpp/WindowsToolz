# WINDOWS TOOLZ

Windows Toolz is a light program to help you fix some broken things.

For now the first version has only a few funcions, but i am planning to add more in the future.

There are two setup files:

# .MSI file

In one you can set a custom path, which allows you to install to whatever folder you like

# .EXE file

It's a basic installation setup, it doesn't come with a custom path, but it also automatically installs VC Redist, as it is required to run the program.

# WHICH FILE SHOULD YOU CHOOSE?

If you want to set up a custom path choose the .MSI file.

The .EXE file automatically installs in the default path (C:\\Program Files (x86)\\Windows Toolz), and comes with the VC Redist package.

# REQUIREMENTS

- Windows 10/11
- VC Redist

# REMINDER

The .MSI installer doesn't install the VC Redist package, so if you don't have it installed and you want to install with the .MSI setup, you need to install it manually. You can get the VC Redist setup here: https://aka.ms/vc14/vc_redist.x64.exe
