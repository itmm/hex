# Meta Test

```
@Def(| cat)
	@put(globals)
@End(| cat)
```

```
@def(print @name())
	@add(globals)
		name
	@end(globals)
@end(print @name())
```

```
@def(globals)
	first
@end(globals)
```

```
@put(print @name(Alice))
put(print @name(Bob))
```


```
@add(globals)
	last
@end(globals)
```
