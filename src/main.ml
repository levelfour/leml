let usage_msg =
  "leML compiler (c) @levelfour_\n" ^
  Printf.sprintf "usage: %s [source-file]\n" Sys.argv.(0)

(* Check if the given file name is valid *)
let validate_filename name =
  if name = "" then
    (false, "specify source file")
  else
    let p = String.rindex name '.' + 1 in
    let ext = String.sub name p ((String.length name) - p) in
    if ext <> "ml" then
      (false, "assembly file name should have extension `.ml`")
    else
      (true, String.sub name 0 (p-1))

let () =
  (* Arguments Parsing *)
  let file = ref "" in
  Arg.parse [] (fun s -> file := s) usage_msg;
  let valid, s = validate_filename !file in
  if not valid then
    let _ = Printf.eprintf "Error: %s\n%s" s usage_msg in exit 1
  else
    file := s;

  (* Opening File *)
  let ic = open_in  (!file ^ ".ml")   in
  let oc = open_out (!file ^ ".s") in

  (* Read Input Source *)
  let buf = Std.input_all ic in
  let lexbuf = Lexing.from_string buf in

  let _  = (Parser.exp Lexer.token lexbuf) in ();

  (* Finalize *)
  close_in  ic;
  close_out oc;
