(*
 * 調和振動の微分方程式
 *   mr'' = -kr
 * をRunge-Kutta法で解き、t=10.0における変位を求める
 *)
let dt = 0.01 in
let m = 20.0 in
let k = 5.0 in
let z = -. k /. m in
let rec fr t r v = v in
let rec fv t r v = z *. r in
let rec solve t r v k1 k2 k3 k4 l1 l2 l3 l4 =
  let k1' = dt *. (fr t r v) in
  let k2' = dt *. (fr (t +. dt/.2.0) (r +. dt/.2.0*.k1) (v +. dt/.2.0*.l1)) in
  let k3' = dt *. (fr (t +. dt/.2.0) (r +. dt/.2.0*.k2) (v +. dt/.2.0*.l2)) in
  let k4' = dt *. (fr (t +. dt) (r +. dt*.k3) (v +. dt*.l3)) in
  let l1' = dt *. (fv t r v) in
  let l2' = dt *. (fv (t +. dt/.2.0) (r +. dt/.2.0*.k1) (v +. dt/.2.0*.l1)) in
  let l3' = dt *. (fv (t +. dt/.2.0) (r +. dt/.2.0*.k2) (v +. dt/.2.0*.l2)) in
  let l4' = dt *. (fv (t +. dt) (r +. dt*.k3) (v +. dt*.l3)) in
  let t' = t +. dt in
  let r' = r +. 1.0/.6.0 *. (k1' +. 2.0*.k2' +. 2.0*.k3' +. k4') in
  let v' = v +. 1.0/.6.0 *. (l1' +. 2.0*.l2' +. 2.0*.l3' +. l4') in
  if 10.0 < t' then r'
  else solve t' r' v' k1' k2' k3' k4' l1' l2' l3' l4'
in (solve 0.0 0.0 1.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0)
