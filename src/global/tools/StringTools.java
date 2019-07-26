package global.tools;

public class StringTools {

    public String beforeParenthesis (String text) {
        int position = 0;
        for (char letter : text.toCharArray()) {
            if (letter == '(') {
                return text.substring(0, position);
            }
            position ++;
        }
        return text;
    }

    public String noSpecialCharacters (String text) {
        String output = "";
        String valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
        boolean flag = true;
        for (char letter : text.toCharArray()) {
            if (valid.indexOf(letter) != -1) {
                output = output.concat(String.valueOf(letter));
                flag = true;
            }else{
                if (flag) {
                    output = output.concat("_");
                    flag = false;
                }
            }
        }
        return output;
    }

}
