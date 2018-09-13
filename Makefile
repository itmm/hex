CFLAGS += -Wall -std=c99

.PHONY: all install clean

all: index.html hex.c

hex.c: index.xml
	hex <index.xml | sed \
		-e 's/&lt;/</g' \
		-e 's/&gt;/>/g' \
		-e 's/&amp;/\&/g' \
		-e 's/b{//g' \
		-e "s/case '''/case '{'/" \
		-e 's/>>$$//' \
		>$@

hex: hex.c

install: hex
	cp hex ~/bin

index.html: index.xml 
	sed	\
		-e 's/1{}/<span class="in1"><\/span>/g' \
		-e 's/2{}/<span class="in2"><\/span>/g' \
		-e 's/3{}/<span class="in3"><\/span>/g' \
		-e 's/4{}/<span class="in4"><\/span>/g' \
		-e 's/5{}/<span class="in5"><\/span>/g' \
		-e 's/b{}/<br\/>/g' \
		-e 's/f{\([^}]*\)}/<span class="fn">\1<\/span>/g' \
		-e 's/v{\([^}]*\)}/<span class="var">\1<\/span>/g' \
		-e 's/s{\([^}]*\)}/<span class="str">\1<\/span>/g' \
		-e 's/n{\([^}]*\)}/<span class="num">\1<\/span>/g' \
		-e 's/t{\([^}]*\)}/<span class="type">\1<\/span>/g' \
		-e 's/k{\([^}]*\)}/<span class="keyword">\1<\/span>/g' \
		-e 's/a{\([^}]*\)}/<span class="add">@add(<span class="macro-name">\1<\/span>)<\/span>/g' \
		-e 's/x{\([^}]*\)}/<span class="end">@end(<span class="macro-name">\1<\/span>)<\/span>/g' \
		-e 's/e{\([^}]*\)}/<span class="expand">@expand(<span class="macro-name">\1<\/span>)<\/span>/g' \
		$^ >$@

clean:
	rm -f hex.c index.html
