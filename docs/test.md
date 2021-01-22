# Meta Test

```c++
@Def(| cat)
	@put(globals)
@End(| cat)
```

```c++
@def(print @name())
	@add(globals)
		name @name()
	@end(globals)
@end(print @name())
```

```c++
@def(globals)
	first
@end(globals)
```

```c++
@put(print @name(Alice))
```

```c++
@put(print @name(Bob))
```

```c++
@add(globals)
	last
@end(globals)
```
