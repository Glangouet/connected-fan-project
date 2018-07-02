<?php
function minifing($input, $output) {
	$search = array (
			"@\r\n@s",
			"@\n@s",
			"@\t@s",
			"@\s*<\s*@s",
			"@\s*>\s*@s",
			"@\s*\(\s*@s",
			"@\s*\)\s*@s",
			"@\s*;\s*@s",
			"@\s*\{\s*@s",
			"@\s*\}\s*@s",
			"@\s*\+\s*@s",
			"@\s*\=\s*@s",
			"@> *<@s",
			"@\"@s" 
	);
	$replace = array (
			" ",
			" ",
			"",
			"<",
			">",
			"(",
			")",
			";",
			"{",
			"}",
			"+",
			"=",
			"><",
			"'" 
	);
	if (! is_file ( $input )) {
		return false;
	}
	$content = file_get_contents ( $input );
	$content = preg_replace ( $search, $replace, $content );
	file_put_contents ( $output, $content );
}
function cleanJs($js_file_content) {
	echo "Removing specific parameters in JavaScript : ";
	$js_file_content = preg_replace ( array (
			"@^var ESP_IP.*\n@m",
			"@ESP_IP \+ @" 
	), array (
			"",
			"" 
	), $js_file_content );
	if (strpos ( $js_file_content, "var ESP_IP" ) !== FALSE || strpos ( $js_file_content, "ESP_IP + " ) !== FALSE) {
		echo "fail ! \n";
		exit ( 1 );
	}
	echo "done.\n";
	return $js_file_content;
}
function generateIndex($html, $css, $js, $minifiedFile) {
	$CSS_EXP = "@<link .*></head>@";
	$JS_EXP = "@<script .*></script>@";
	$js_content = "<script type='text/javascript' >" . file_get_contents ( $js ) . "</script>";
	$css_content = "<style>" . file_get_contents ( $css ) . "</style></head>";
	$minified_content = preg_replace ( array (
			$CSS_EXP,
			$JS_EXP 
	), array (
			$css_content,
			$js_content 
	), file_get_contents ( $html ) );
	file_put_contents ( $minifiedFile, $minified_content );
}
function pasteInCppFile($minifiedFile, $cppFile, $functionName) {
	$html = file_get_contents ( $minifiedFile );
	$cpp = file_get_contents ( $cppFile );
	$matches = array ();
	preg_match ( "@.*" . $functionName . "\(\)\s*{\s*(.*)\s*}\s*@s", $cpp, $matches );
	$old_content = $matches [1];
	$arguments = explode ( ",", $old_content );
	array_splice ( $arguments, 0, 2 );
	$argument = trim ( implode ( ",", $arguments ) );
	$argument = substr ( $argument, 0, strlen ( $argument ) - 2 );
	$cpp = preg_replace ( "@F(\"(.*)\")@", $argument, $cpp );
	file_put_contents ( $cppFile, str_replace ( $argument, "F(\"$html\")", $cpp ) );
}
function displayStat($files, $before, $after) {
	$saved = sizeof ( $files ) * 100;
	for($i = 0; $i < sizeof ( $files ); $i ++) {
		$b = $before [$i];
		$a = $after [$i];
		$save = number_format ( ($a * 100 / $b), 2 );
		$saved -= $save;
		echo "Minified " . $files [$i] . " from " . $b . " to " . $a . " (" . $save . "%)\n";
	}
	echo "Total saved : " . $saved . "%\n";
}
function main() {
	$minified_folder = "minified";
	$normal_folder = "normal";
	$afterLen = array ();
	$beforeLen = array ();
	$files = array (
			"index.html",
			"esp/style.css",
			"esp/script.js" 
	);
	// Minifying all files
	foreach ( $files as $file ) {
		$input = $normal_folder . '/' . $file;
		$output = $minified_folder . '/' . $file;
		if (pathinfo ( $input ) ['extension'] == "js") {
			file_put_contents ( $output, cleanJs ( file_get_contents ( $input ) ) );
			minifing ( $output, $output );
		} else {
			minifing ( $input, $output );
		}
		$beforeLen [] = strlen ( file_get_contents ( $input ) );
		$afterLen [] = strlen ( file_get_contents ( $output ) );
	}
	// Generating minified content
	$minifiedFile = "index-all-minified.html";
	generateIndex ( $minified_folder . '/' . $files [0], $minified_folder . '/' . $files [1], $minified_folder . '/' . $files [2], $minifiedFile );
	// Copying content in cpp file
	$cppFile = "../../libraries/HttpConfigServer/src/EspHttpServer.cpp";
	pasteInCppFile ( $minifiedFile, $cppFile, "displayHtml" );
	// Displaying statistics
	displayStat ( $files, $beforeLen, $afterLen );
}

main ();

?>