CRGB findColorClosestToMean(const CRGB arr[], int size) {
  if (size == 0) {
    // Handle the case when the array is empty.
    return CRGB::White;  // Return a default value or throw an exception.
  }

  // Calculate the mean of the array.
  double mean_1 = 0;
  double mean_2 = 0;
  double mean_3 = 0;

  for (int i = 0; i < size; i++) {
    mean_1 += arr[i].red;
    mean_2 += arr[i].green;
    mean_3 += arr[i].blue;
  }
  mean_1 /= size;
  mean_2 /= size;
  mean_3 /= size;

  // Find the element with the smallest absolute difference to the mean.
  CRGB closestElement = arr[0];
  uint8_t smallestDiff = 255;
  for (int i = 0; i < size; i++) {
    double diff = abs(arr[i].red - mean_1) + abs(arr[i].green - mean_2) + abs(arr[i].blue - mean_3);
    if (diff < smallestDiff) {
      smallestDiff = diff;
      closestElement = arr[i];
    }
  }

  return closestElement;
}