package application;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Utils {
	private Utils() {}
	
	static List<String> splitAtNthOccurrence(String input, int n, char delimiter) {
	    List<String> pieces = new ArrayList<>();
	    int k = 0;
	    int startPoint = 0;
	    for (int i = 0; i < input.length(); i++)
	    {
	      if (input.charAt(i) == delimiter)
	      {
	          k++;
	          if (k == n)
	          {
	              String ab = input.substring(startPoint, i+1);
	              System.out.println(ab);
	              pieces.add(ab);
	              startPoint = i+1;
	              k = 0;
	          }
	      }
	    }
	    return pieces;
	}
}
