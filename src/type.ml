type t =
  | Unit
  | Int
  | Float
  | Bool
  | Fun of t list * t
  | Tuple of t list
  | Array of t
  | Var of t option ref

(* generate new type variable (e.g. 'a ) *)
let gentyp () = Var(ref None)
