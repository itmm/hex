# Buffer
* Ein Buffer ist eine Erweiterung zum C++ String
* Er kann aktiv oder inaktiv sein
* Und er hält Datei-Namen und Zeilennummern des enthaltenen Bereichs

```
A{global elements}
	e{define buf};
x{global elements}
```
* Buffer sind global sichtbare Strukturen

```
d{define buf}
	class Buf {
		std::string _value;
		std::string _file;
		int _startLine;
		int _endLine;
		bool _active;
		e{private methods};
	public:
		e{buf methods};
	};
x{define buf}
```

```
d{buf methods}
	Buf(): _active(false) {}
x{buf methods}
```

```
a{buf methods}
	bool active() const {
		return _active;
	}
x{buf methods}
```

```
a{buf methods}
	bool empty() const {
		return _value.empty();
	}
x{buf methods}
```

```
a{buf methods}
	void activate() {
		_active = true;
	}
x{buf methods}
```

```
a{buf methods}
	void clear(bool active = false) {
		_value.clear();
		_file.clear();
		_startLine = 0;
		_endLine = 0;
		_active = active;
	}
x{buf methods}
```

```
a{buf methods}
	const std::string &str() const {
		return _value;
	}
x{buf methods}
```

```
a{buf methods}
	const std::string &file() const {
		return _file;
	}
x{buf methods}
```

```
a{buf methods}
	int startLine() const {
		return _startLine;
	}
x{buf methods}
```

```
a{buf methods}
	int endLine() const {
		return _endLine;
	}
x{buf methods}
```

```
d{private methods}
	bool _canContinue(
		const std::string &file,
		int line
	) {
		if (_file != file) {
			return false;
		}
		return 
			line == _endLine ||
			line == _endLine + 1;
	}
x{private methods}
```

```
a{buf methods}
	bool canContinue(
		const std::string &file,
		int line
	) {
		if (_file.empty()) {
			return true;
		}
		return _canContinue(
			file, line
		);
	}
x{buf methods}
```

```
a{private methods}
	void assertCont(
		const std::string &file,
		int line
	) {
		if (_file.empty()) {
			_file = file;
			_startLine = line;
			_endLine = line;
		}
		ASSERT(_canContinue(file, line));
	}
x{private methods}
```

```
a{buf methods}
	void add(
		const std::string &value,
		const std::string &file =
			"/dev/null",
		int line = 1
	) {
		activate();
		if (value.empty()) { return; }
		assertCont(file, line);
		_value += value;
		_endLine = line;
	}
x{buf methods}
```

```
a{buf methods}
	void add(
		int ch,
		const std::string &file =
			"/dev/null",
		int line = 1
	) {
		char c = static_cast<char>(ch);
		add(
			std::string(&c, &c + 1),
			file, line
		);
	}
x{buf methods}
```

```
a{buf methods}
	void add(const Buf &b) {
		activate();
		if (b.empty()) { return; }
		assertCont(
			b._file, b._startLine
		);
		_value += b._value;
		_endLine = b._endLine;
	}
x{buf methods}
```

# Unit Tests

```
D{perform unit-tests}
	e{unit tests};
x{perform unit-tests}
```

```
d{unit tests} {
	Buf b;
	ASSERT(! b.active());
} x{unit tests}
```

```
a{unit tests} {
	Buf b;
	b.activate();
	ASSERT(b.active());
} x{unit tests}
```

```
a{unit tests} {
	Buf b;
	b.activate();
	b.clear();
	ASSERT(! b.active());
} x{unit tests}
```

```
d{foo entry}
	Buf b;
	b.add("abc", "foo.x", 1);
x{foo entry}
```

```
a{unit tests} {
	E{foo entry};
	ASSERT(b.active());
} x{unit tests}
```

```
a{unit tests} {
	E{foo entry};
	ASSERT(b.str() == "abc");
} x{unit tests}
```

```
a{unit tests} {
	E{foo entry};
	ASSERT(b.file() == "foo.x");
} x{unit tests}
```

```
a{unit tests} {
	E{foo entry};
	ASSERT(b.startLine() == 1);
} x{unit tests}
```

```
a{unit tests} {
	E{foo entry};
	ASSERT(b.endLine() == 1);
} x{unit tests}
```

```
d{foo two lines}
	E{foo entry};
	b.add("\n", "foo.x", 1);
	b.add("def", "foo.x", 2);
x{foo two lines}
```

```
a{unit tests} {
	E{foo two lines};
	ASSERT(b.str() == "abc\ndef");
} x{unit tests}
```

```
a{unit tests} {
	E{foo two lines};
	ASSERT(b.startLine() == 1);
} x{unit tests}
```

```
a{unit tests} {
	E{foo two lines};
	ASSERT(b.endLine() == 2);
} x{unit tests}
```

```
d{bar entry}
	E{foo two lines};
	Buf c;
	c.add(b);
x{bar entry}
```

```
a{unit tests} {
	E{bar entry};
	ASSERT(c.str() == b.str());
} x{unit tests}
```

```
a{unit tests} {
	E{bar entry};
	ASSERT(
		c.startLine() ==
			b.startLine()
	);
} x{unit tests}
```

```
a{unit tests} {
	E{bar entry};
	ASSERT(
		c.endLine() ==
			b.endLine()
	);
} x{unit tests}
```

```
d{bar two entry}
	E{bar entry};
	Buf d;
	d.add("\n", "foo.x", 2);
	d.add("ghi", "foo.x", 3);
	c.add(d);
x{bar two entry}
```

```
a{unit tests} {
	E{bar two entry}
	ASSERT(
		c.str() ==
			"abc\ndef\nghi"
	);
} x{unit tests}
```

```
a{unit tests} {
	E{bar two entry}
	ASSERT(c.startLine() == 1);
} x{unit tests}
```

```
a{unit tests} {
	E{bar two entry}
	ASSERT(c.endLine() == 3);
} x{unit tests}
```

