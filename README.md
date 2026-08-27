# WINDOWS TOOLZ

Windows Toolz is a light program to help you fix some broken things.

For now the first version has only one funcion, but i am planning to add more in the future.

There are two setup files:

# .MSI file

It has a feature tree, which allows you to configure what features do you want to install. 

# .EXE file

It's a basic installation setup, it doesn't come with a feature tree, but it also automatically installs VC Redist, as it is required to run the program.

# WHICH FILE SHOULD YOU CHOOSE?

If you want to choose to have a shortcut on your Desktop or in the Start Menu, choose the .MSI file.

The .EXE file automatically adds the shortcuts, and comes with the VC Redist package.

# REQUIREMENTS

- Windows 10/11
- VC Redist

# REMINDER

The .MSI installer doesn't install the VC Redist package, so if you don't have it installed and you want to install with the .MSI setup, you need to install it manually. You can get the VC Redist setup here: https://aka.ms/vc14/vc_redist.x64.exe
