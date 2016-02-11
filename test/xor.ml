let rec xor x y = if x then not y else y in
print_bool (xor true false)
