./distvec mp3_topology$1.txt mp3_messages$1.txt mp3_changes$1.txt
mv output.txt output$1_distvec.txt
echo "****distvec finished****"
echo ""
./linkstate mp3_topology$1.txt mp3_messages$1.txt mp3_changes$1.txt
mv output.txt output$1_linkstate.txt
echo "****linkstate finished****"
echo ""
diff output$1_distvec.txt output$1_linkstate.txt > output_diff.txt
diffCount=$(wc -l output_diff.txt)
echo $diffCount
mv output_diff.txt output$1_diff.txt
