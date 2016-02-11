let x =
  let y = create_array 1 3.14 in
  create_array 10 y
in print_float (x.(0).(0))
