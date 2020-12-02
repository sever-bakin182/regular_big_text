My info:
-name  - Bakin Sergej
-email - severbt@gmail.com

Desription:
	Implmented the class CLogReader.
	The function CheckStr() is a base of this class .
	It's simple implementation of a regular expression.
	The value of the MAXSIZE_DATA is 4096. It's size
	of a cluster type of the NTFS.
	
	All functions have understand name so I didn't write
    many descriptions.

	One checking in the function SetNextStart can remove
	because different end of a file and end of a string
	cannot be less then 0.
	
	All conditons in the task completed. Also added
	a symbol #.
		# - only number.
		
	All input variables have first symbol "_". For
	example _buf.

Examples use a log_reader:
	log_reader.exe order.txt order*closed
	log_reader.exe order.txt order*
	log_reader.exe order.txt *order?
	log_reader.exe order.txt ##*order?
	
	
Links of some tests:
StrLen()    - https://ideone.com/MRekcq
RStrLen()   - https://ideone.com/6FCjjv
SetFilter() - https://ideone.com/5vqj2C