idf_component_register(SRCS "rep.c" "gen.c" "vault.c" "spref.c" "utils.c" "main.c"
                    INCLUDE_DIRS ".")
idf_build_set_property(PARTITION_TABLE_FILENAME "partitions.csv" PROPERTY)
