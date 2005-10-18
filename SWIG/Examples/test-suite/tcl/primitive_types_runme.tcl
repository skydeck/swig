
if [ catch { load ./primitive_types[info sharedlibextension] primitive_types} err_msg ] {
	puts stderr "Could not load shared object:\n$err_msg"
}


if {[val_int 10] != 10 }  { error "bad int map"  }
if {[val_schar 10] != 10 } { error "bad char map" }
if {[val_short 10] != 10 } { error "bad schar map" }


if [catch { val_schar 10000 } ] {} else { error "bad schar map" }
if [catch { val_uint  -100 } ]  {} else { error "bad uint map"  }
if [catch { val_uchar -100 } ]  {} else { error "bad uchar map" }

if {[val_uint 10] != 10 }  { error "bad uint map"  }
if {[val_uchar 10] != 10 } { error "bad uchar map" }
if {[val_ushort 10] != 10 } { error "bad ushort map" }



