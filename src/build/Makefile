all:
	./mk
clean:
	killall -q crystal || true
	find .. -type d | grep -E -v 'build' | xargs rm -rf 2>/dev/null || true
	#build|utils' || exit 0

install:
	chmod +x ../utilities/*
	cp ../utilities/* /usr/local/bin
	#cp ../utilities/* /usr/local/bin
	#cp  ../utilities/* /usr/local/bin /usr/bin
