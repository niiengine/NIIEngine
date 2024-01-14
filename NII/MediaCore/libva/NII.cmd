NII_PRE_COMMAND
    bzip -d -k mpg123-1.14.2.tar.bz2
    tar -xf mpg123-1.14.2.tar
NII_PRE_COMMAND_END
     
NII_CONFIGURE
    ./mpg123-1.14.2/configure
NII_CONFIGURE_END

NII_MAKE
    ./mpg123-1.14.2/make
NII_MAKE_END

NII_MAKEINSTALL
    ./mpg123-1.14.2/make install
NII_MAKEINSTALL_END