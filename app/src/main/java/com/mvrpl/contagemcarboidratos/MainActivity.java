package com.mvrpl.contagemcarboidratos;

import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.TextView;

import java.util.HashMap;
import java.util.Map;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        ConstraintLayout telaPrincipal = (ConstraintLayout) findViewById(R.id.tela_principal);

        final TextView resultado = (TextView)findViewById(R.id.resultadoUI);

        final EditText valorCarboidratos = (EditText)findViewById(R.id.valorCarboidratos);
        final EditText uiCarbo = (EditText)findViewById(R.id.valorDePara);
        final EditText valorGlicemia = (EditText)findViewById(R.id.valorGlicemia);
        final EditText glicemiaAlvo = (EditText)findViewById(R.id.valorGA);
        final EditText fatorSens = (EditText)findViewById(R.id.valorFS);

        Map<String, EditText> confs = new HashMap<String, EditText>();
        confs.put("uiCarbo", uiCarbo);
        confs.put("glicemiaAlvo", glicemiaAlvo);
        confs.put("fatorSens", fatorSens);

        final Context contexto = this;

        for (final Map.Entry<String, EditText> e : confs.entrySet()) {
            e.getValue().setText(obterValor(contexto, e.getKey()));

            e.getValue().setOnFocusChangeListener(new View.OnFocusChangeListener() {
                @Override
                public void onFocusChange(View v, boolean hasFocus) {
                    if (!hasFocus) {
                        guardarValor(contexto, e.getKey(), e.getValue().getText().toString());
                    }
                }
            });
        }

        telaPrincipal.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (valorCarboidratos.getText().toString().length() > 0 && valorGlicemia.getText().toString().length() > 0) {
                    String res = Integer.toString(
                            (int) Math.ceil(
                                    ((Float.parseFloat(valorGlicemia.getText().toString()) - Float.parseFloat(glicemiaAlvo.getText().toString())) / Float.parseFloat(fatorSens.getText().toString()))
                                            + (Float.parseFloat(valorCarboidratos.getText().toString()) / Float.parseFloat(uiCarbo.getText().toString())))
                    );
                    resultado.setText(res);

                }
                hideKeyboard(view);
            }
        });

    }

    public void hideKeyboard(View view) {
        InputMethodManager inputMethodManager =(InputMethodManager)getSystemService(Activity.INPUT_METHOD_SERVICE);
        inputMethodManager.hideSoftInputFromWindow(view.getWindowToken(), 0);
    }

    public static String obterValor(Context context, String chave) {
        SharedPreferences confs = context.getSharedPreferences("ContagemCarboidratos", Context.MODE_PRIVATE);
        return confs.getString(chave, "");
    }

    public static void guardarValor(Context context, String chave, String valor) {
        SharedPreferences confs = context.getSharedPreferences("ContagemCarboidratos", Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = confs.edit();
        editor.putString(chave, valor);
        editor.commit();
    }
}
