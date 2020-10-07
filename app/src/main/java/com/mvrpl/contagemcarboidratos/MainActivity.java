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
        final EditText valorGorduras = (EditText)findViewById(R.id.valorGorduras);
        final EditText valorProteinas = (EditText)findViewById(R.id.valorProteinas);
        final EditText uiCarbo = (EditText)findViewById(R.id.valorDePara);
        final EditText valorGlicemia = (EditText)findViewById(R.id.valorGlicemia);
        final EditText glicemiaAlvo = (EditText)findViewById(R.id.valorGA);
        final EditText fatorSens = (EditText)findViewById(R.id.valorFS);
        final EditText fatorGord = (EditText)findViewById(R.id.valorPercentGord);
        final EditText fatorProte = (EditText)findViewById(R.id.valorPercentProte);

        Map<String, EditText> confs = new HashMap<String, EditText>();
        confs.put("uiCarbo", uiCarbo);
        confs.put("glicemiaAlvo", glicemiaAlvo);
        confs.put("fatorSens", fatorSens);
        confs.put("fatorGord", fatorGord);
        confs.put("fatorProte", fatorProte);

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
                if (valorCarboidratos.getText().toString().length() > 0 && valorGlicemia.getText().toString().length() > 0 && valorGorduras.getText().toString().length() > 0 && valorProteinas.getText().toString().length() > 0) {
                    int correcao = (int) Math.ceil(((Float.parseFloat(valorGlicemia.getText().toString()) - Float.parseFloat(glicemiaAlvo.getText().toString())) / Float.parseFloat(fatorSens.getText().toString())));
                    int carboidratos_ui = (int) Math.ceil((Float.parseFloat(valorCarboidratos.getText().toString()) / Float.parseFloat(uiCarbo.getText().toString())));
                    int gorduras_ui = (int) Math.ceil( ((Float.parseFloat(fatorGord.getText().toString()) / 100) * Float.parseFloat(valorGorduras.getText().toString())) / Float.parseFloat(uiCarbo.getText().toString()) );
                    int proteinas_ui = (int) Math.ceil( ((Float.parseFloat(fatorProte.getText().toString()) / 100) * Float.parseFloat(valorProteinas.getText().toString())) / Float.parseFloat(uiCarbo.getText().toString()) );
                    String res = Integer.toString((int) Math.ceil(correcao + carboidratos_ui + gorduras_ui + proteinas_ui));
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
