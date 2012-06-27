libejson, (C) Michel Pollet <buserror@gmail.com>
https://github.com/buserror-uk/libejson

Introduction
--------
libejson is a library designed to parse JSON text (http://json.org/), as well as EJSON, 
an "extended" version of JSON that was designed to be more friendly for settings files, 
storage, and other uses like human interface description files.

libejson uses a simple "driver" (collection of callbacks) system that is very similar
to libexpat for XML, it parses the input and repeatedly calls your callbacks; your 
code is responsible for storing, allocating  and generally "do stuff" with the parsed data.

libejson doesn't allocate memory generally, it will allocate a 128 byte buffer on 
the stack if it decodes base64 data (see below), otherwise it's footprint is minimal.

Extensions
--------
"Extended" JSON implements these extensions:

* **EJSON makes quoting node identifiers optional, if they don't contain spaces.**
  Normal JSON forces you to use syntax like `{ "hello" : "world" }` in EJSON, you can use 
  `{ hello : "world" }`. This makes it easier to read/edit settings files by hand.
* **EJSON nodes can have flags/attributes**
  You can specify a list of words as a node "attribute". This allows flags to be passed 
  to the host program for this particular node. It's especially important for files that 
  describe human interface, or to allow easier debugging in some cases.
  
  Syntax Example: `{ node_label (visible, translated) : "Bonjour" }`
* **EJSON explicitly allows trailing comma in lists**
  JSON does not allow trailing comma in lists, which makes code that generate JSON more 
  complicated for no reason, and also makes "patching" JSON files more difficult. 
  EJSON allows trailing commas in objects and arrays. 
  
  Syntax Example: `[0,1,2,]`
* **EJSON allows binary data values**
  You can have arbitrary binary data in nodes, encoded in base64. This is often very 
  useful to store small pieces of binary, small images, or other things embedded in 
  a bigger file. The base64 values are delimited by % (percent) characters.
  
  Syntax Example: `{ node : % WW91IGNhbiBlaXRoZXIgdXBsb2FkIGEgZmlsZ== % }`
* **EJSON allows hexadecimal integer constants**
  hex constants are essential for files edited by hand.
  
  Syntax Example: `{ node : 0xdeadbeef }`

Example:
--------
	{
		device : [
		{
			name :  "Keyboard",
			label (visible) : "105 keys keyboard",
			flags : 0xcafef00d,
			icon : %
				RGVjb2RlIGJhc2U2NCBzdHJpbmdzIChiYXNlNjQgc3RyaW5nIGxvb2tzIGxpa2UgWVRNME5ab21J
				ekkyT1RzbUl6TTBOVHVlWVE9PSkNCkRlY29kZSBhIGJhc2U2NCBlbmNvZGVkIGZpbGUgKGZvciBl
				eGFtcGxlIElDTyBmaWxlcyBvciBmaWxlcyBmcm9tIE1JTUUgbWVzc2FnZSkNCkNvbnZlcnQgc291
				cmNlIHRleHQgZGF0YSBmcm9tIHNldmVyYWwgY29kZSBwYQ== %
		},
		]
	}
