(* thanks to http://ameblo.jp/nuevo-namasute/entry-10006785787.html
   and http://blog.livedoor.jp/azounoman/archives/50232574.html *)
let rec f n =
  if n < 0 then 0 else
   let a = Array.create 1 f in
   (a.(0) (n - 1)) + n in
f 9
