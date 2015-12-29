let rec mdb n a b =
  let rec mdb_sub m x y =
    if n = m then (x, y)
    else
      mdb_sub (m + 1)
      (x *. x -. y *. y +. a)
      (2.0 *. x *. y +. b)
  in mdb_sub 0 0.0 0.0 in
let (x, y) = (mdb 5 1.0 0.0) in
int_of_float x
