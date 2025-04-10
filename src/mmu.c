#include <stdio.h>
#include <stdlib.h>

/** Geospatial Data Abstraction Library (GDAL) **/
#include "gdal.h"       // public (C callable) GDAL entry points
#include "cpl_conv.h"   // various convenience functions for CPL
#include "cpl_string.h" // various convenience functions for strings


#include "utils/const.h"
#include "utils/alloc.h"
#include "utils/conncomp.h"
#include "utils/dir.h"
#include "utils/float.h"
#include "utils/string.h"


enum { __binary__ = 0, __multi__ = 1 };

typedef struct {
  char input_path[STRLEN];
  char output_path[STRLEN];
  int min_size;
  int input_band;
  int type;
} args_t;


void usage(char *exe, int exit_code){

  printf("Usage: %s [-b <input_band>] [-t binary|multi]\n", exe);
  printf("       <input-path> <output-path> <min-size>\n");
  printf("  \n");
  printf("  -b = band of input image, default: 1\n");
  printf("  -t = type, must be binary or multi, default: multi\n");
  printf("       binary means that the pixel values are ignored, hence connected objects are computed from a binary representation of the image     where all valid pixels are considered as objects\n");
  printf("       If the input image is float, only binary is allowed\n");
  printf("  \n");
  printf("  input-path: input image\n");
  printf("  output-path: output image\n");
  printf("  min-size: minimum object size in pixels\n");

  printf("\n");

  exit(exit_code);
  return;
}

void parse_args(int argc, char *argv[], args_t *args){
int opt;

  opterr = 0;

  args->input_band = 1;
  args->type = __binary__;

  while ((opt = getopt(argc, argv, "b:t:")) != -1){
    switch(opt){
      case 'b':
        args->input_band = atoi(optarg);
        if (args->input_band < 1){
          fprintf(stderr, "input band must be >= 1\n");
          usage(argv[0], FAILURE);  
        }
        break;
      case 't':
        if (strcmp(optarg, "binary")){
          args->type = __binary__;
        } else if (strcmp(optarg, "multi")){
          args->type = __multi__;
          printf("warning: type `multi` is not implemented yet, type `binary` is used instead\n");
          args->type = __binary__;
        } else {
          fprintf(stderr, "unkwnon type. Use `binary` or `multi`\n");
          usage(argv[0], FAILURE);  
        }
        break;
       case '?':
        if (isprint(optopt)){
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        } else {
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        usage(argv[0], FAILURE);
      default:
        fprintf(stderr, "Error parsing arguments.\n");
        usage(argv[0], FAILURE);
    }
  }


  int mandatory = 3;

  if (optind < argc){
    if (argc-optind == mandatory){
      copy_string(args->input_path, STRLEN, argv[optind++]);
      if (!fileexist(args->input_path)){
        fprintf(stderr, "Input image does not exist: %s\n", args->input_path);
        usage(argv[0], FAILURE);
      }
      copy_string(args->output_path, STRLEN, argv[optind++]);
      if (fileexist(args->output_path)){
        fprintf(stderr, "Output image already exists: %s\n", args->output_path);
        usage(argv[0], FAILURE);
      }
      args->min_size = atoi(argv[optind++]);
      if (args->min_size < 1){
        fprintf(stderr, "minimum size must be >= 1\n");
        usage(argv[0], FAILURE);
      }
    } else if (argc-optind < mandatory){
      fprintf(stderr, "some non-optional arguments are missing.\n");
      usage(argv[0], FAILURE);
    } else if (argc-optind > mandatory){
      fprintf(stderr, "too many non-optional arguments.\n");
      usage(argv[0], FAILURE);
    }
  } else {
    fprintf(stderr, "non-optional arguments are missing.\n");
    usage(argv[0], FAILURE);
  }


  return;
}


int main ( int argc, char *argv[] ){


  args_t args;

  parse_args(argc, argv, &args);


  GDALAllRegister();

  GDALDatasetH  input_dataset;
  GDALRasterBandH input_band;
  GDALDataType input_datatype;
  int nrow, ncol, ncell, nband;
  char projection[STRLEN];
  double geotransformation[6];
  double nodata;
  int has_nodata;


  if ((input_dataset = GDALOpen(args.input_path, GA_ReadOnly)) == NULL){ 
    fprintf(stderr, "could not open %s\n", args.input_path); 
    exit(FAILURE);
  }
  
  
  
  ncol  = GDALGetRasterXSize(input_dataset);
  nrow  = GDALGetRasterYSize(input_dataset);
  ncell = ncol*nrow;
  
  copy_string(projection, STRLEN, GDALGetProjectionRef(input_dataset));
  GDALGetGeoTransform(input_dataset, geotransformation);


  nband = GDALGetRasterCount(input_dataset);
  if (args.input_band > nband){
    fprintf(stderr, "illegal band number (%d of %d bands requested)\n", args.input_band, nband); 
    exit(FAILURE);
  }
  input_band = GDALGetRasterBand(input_dataset, args.input_band);

  nodata = GDALGetRasterNoDataValue(input_band, &has_nodata);
  if (!has_nodata){
    fprintf(stderr, "%s has no nodata value.\n", args.input_path); 
    exit(FAILURE);
  }

  input_datatype = GDALGetRasterDataType(input_band);
  if (input_datatype > GDT_Float32){
    printf("datatype should not exceed a Float32 (is: %s)\n", GDALGetDataTypeName(input_datatype)); 
    exit(FAILURE);
  }

  printf("projection: %s\n", projection);
  printf("origin: %.6f %.6f\n", geotransformation[0], geotransformation[3]);
  printf("resolution: %.6f %.6f\n", geotransformation[1], geotransformation[5]);
  printf("dimensions: %d x %d = %d pixels\n", nrow, ncol, ncell);
  printf("nodata: %f\n", nodata);
  printf("datatype: %s\n", GDALGetDataTypeName(input_datatype));


  float *input_image = NULL;
  alloc((void**)&input_image, ncell, sizeof(float));
  
  if (GDALRasterIO(input_band, GF_Read, 0, 0, ncol, nrow, input_image, ncol, nrow, GDT_Float32, 0, 0) == CE_Failure){
    printf("could not read data from %s\n", args.input_path); 
    exit(FAILURE);
  }
    
  GDALClose(input_dataset);


  bool *binary_image = NULL;
  alloc((void**)&binary_image, ncell, sizeof(bool));
  
  for (int c=0; c<=ncell; c++){
    if (!fequal(input_image[c], nodata)) binary_image[c] = true;
  }
  
  
  int object_number;
  int *segmented_image = NULL;
  alloc((void**)&segmented_image, ncell, sizeof(int));
  
  binary_image[0] = false; // to ensure that 1st object gets ID=1
  object_number = connectedcomponents_(binary_image, segmented_image, ncol, nrow);
  
  if (object_number > 0){
    
    int object_id;
    int *object_sizes = NULL;
    alloc((void**)&object_sizes, object_number+1, sizeof(int));

    for (int c=0; c<ncell; c++){
      if (binary_image[c]){
        object_id = segmented_image[c]; 
        object_sizes[object_id]++;
      }
    }

    for (int c=0; c<ncell; c++){
      if (binary_image[c]){
        object_id = segmented_image[c]; 
        if (object_sizes[object_id] < args.min_size) input_image[c] = nodata;
      }
    }

    free((void*)object_sizes);

  }

  free((void*)segmented_image);
  free((void*)binary_image);



  GDALDatasetH output_dataset = NULL;
  GDALRasterBandH output_band = NULL;
  GDALDriverH output_driver = NULL;
  char **output_options = NULL;

  if ((output_driver = GDALGetDriverByName("GTiff")) == NULL){
    printf("%s driver not found\n", "GTiff"); 
    exit(FAILURE);
  }

  output_options = CSLSetNameValue(output_options, "COMPRESS", "ZSTD");
  output_options = CSLSetNameValue(output_options, "PREDICTOR", "2");
  output_options = CSLSetNameValue(output_options, "BIGTIFF", "YES");
  output_options = CSLSetNameValue(output_options, "OVERVIEWS", "NONE");


  if ((output_dataset = GDALCreate(output_driver, args.output_path, ncol, nrow, 1, input_datatype, output_options)) == NULL){
    printf("Error creating file %s.\n", args.output_path);
    exit(FAILURE);
  }

  output_band = GDALGetRasterBand(output_dataset, 1);

  
  if (GDALRasterIO(output_band, GF_Write, 0, 0, ncol, nrow, input_image, ncol, nrow, GDT_Float32, 0, 0) == CE_Failure){
    printf("Unable to write %s.\n", args.output_path); 
    exit(FAILURE);
  }

  GDALSetDescription(output_band, "MMU-filtered image");
  GDALSetRasterNoDataValue(output_band, nodata);

  GDALSetGeoTransform(output_dataset, geotransformation);
  GDALSetProjection(output_dataset,   projection);

  GDALClose(output_dataset);



  free((void*)input_image);
  if (output_options != NULL) CSLDestroy(output_options);   

  return SUCCESS; 
}

