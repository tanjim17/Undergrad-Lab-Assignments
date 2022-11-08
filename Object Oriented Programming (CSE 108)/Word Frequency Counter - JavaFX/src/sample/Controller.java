package sample;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.stage.FileChooser;

import java.io.*;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Controller implements Initializable {

    @FXML private TextField textField;
    @FXML private TableView<Words> table;
    @FXML private TableColumn<Words,String> Col1;
    @FXML private TableColumn<Words,Integer> Col2;
    ObservableList<Words> data = FXCollections.observableArrayList();
    File file = null;
    String filename;

    public void initialize(URL location, ResourceBundle resources){
        Col1.setCellValueFactory(new PropertyValueFactory<>("word"));
        Col2.setCellValueFactory(new PropertyValueFactory<>("frequency"));
        table.setItems(data);
    }

    public void openFileChooser(ActionEvent event){
        FileChooser fc = new FileChooser();
        file = fc.showOpenDialog(null);

        if(file != null){
            filename = file.getName();
            textField.setText(filename);
        }
    }

    public void showFreq(ActionEvent event) throws IOException {

        data.clear();

        if (filename.endsWith(".txt")) {

            //Reading file to a string
            BufferedReader in = new BufferedReader(new FileReader(file));
            String temp;
            StringBuilder str = new StringBuilder();

            while ((temp = in.readLine()) != null) {
                str.append(temp).append(" ");
            }
            String input = str.toString();

            //Extracting words
            Pattern p = Pattern.compile("[A-Za-z']+");
            Matcher m = p.matcher(input);
            List<String> list = new ArrayList<>();

            while (m.find()) {
                list.add(input.substring(m.start(), m.end()));
            }

            //Counting and data entry
            int count;
            boolean found = false;
            List<String> repeatList = new ArrayList<>();

            for(int i=0; i<list.size(); i++){

                for(int j=0; j<repeatList.size(); j++){
                    if( list.get(i).equalsIgnoreCase(repeatList.get(j)) ){
                        found = true;
                        break;
                    }
                }
                if(found){
                    found = false;
                    continue;
                }

                count = 1;
                for(int j=i+1; j<list.size(); j++){
                    if( list.get(i).equalsIgnoreCase(list.get(j)) ){
                        count++;
                    }
                }
                data.add(new Words(list.get(i),count));
                repeatList.add(list.get(i));
            }
        }
    }
}