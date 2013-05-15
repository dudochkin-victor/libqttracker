#!/usr/bin/awk -f
next_is_new || NR == 1 { prev = $1; start = $1; next_is_new = 0; }
$2 == "--" { next_is_new = 1; next; }
{
	print $1 - start "\t +" $1 - prev "\t" $0;
	prev = $1;
}
