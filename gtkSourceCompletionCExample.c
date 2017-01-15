#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>
#include <stdlib.h>

G_MODULE_EXPORT void on_aplicacao_sair(GSimpleAction *action, GVariant *parameter, GtkApplication *app);
G_MODULE_EXPORT void on_aplicacao_ativada(GApplication *app);
G_MODULE_EXPORT gboolean on_areaTexto_show_completion(GtkSourceView *view, gpointer *dadosUsuario);
GtkTextIter *obterPosicaoCursor(GtkWidget *areaTexto);

int main(int argc, char **argv) {
  GtkApplication *app;

  // Criar a aplicacao
  app = gtk_application_new("org.gtk.examploSourceCompletionC", 0);
  // Acoes basicas
  GActionEntry app_entries[] = {
    { "quit", on_aplicacao_sair, NULL, NULL, NULL }
  };

  // Adicionar acoes na aplicacao
  g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);
  // Conectar o sinal de ativacao da aplicacao
  g_signal_connect(app, "activate", G_CALLBACK(on_aplicacao_ativada), NULL);

  // Rodar aplicacao
  g_application_run(G_APPLICATION(app), 0, NULL);

  return 0;
}

// Funcao para sair da aplicacao
G_MODULE_EXPORT void on_aplicacao_sair(GSimpleAction *action, GVariant *parameter, GtkApplication *app) {
  GtkWidget *jan;
  GList *lista, *prox;

  // Obter a lista com todas janelas da aplicacao
  lista = gtk_application_get_windows(app);
  while (lista) {
    // Obter a janela da posicao atual da lista
    jan = lista->data;
    // Apontar para a prox posicao da lista
    prox = lista->next;

    // Destruir a janela
    gtk_widget_destroy(GTK_WIDGET(jan));

    // Fazer a lista apontar pra prox posicao
    lista = prox;
  }
}

// Funcao para quando a aplicacao for ativada
G_MODULE_EXPORT void on_aplicacao_ativada(GApplication *app) {
  GtkWidget *jan;

  // Criar nova janela para a aplicacao
  jan = gtk_application_window_new(GTK_APPLICATION(app));
  // Definir tamanho da janela
  gtk_window_set_default_size(GTK_WINDOW(jan), 200, 200);

  // Obter gerenciador de lingugens padrao
  GtkSourceLanguageManager *lm = gtk_source_language_manager_get_default();
  // Obter linguagem C
  GtkSourceLanguage *cLang = gtk_source_language_manager_get_language(lm, "c");
  // Criar buffer com a linguagem C definida por padrao
  GtkSourceBuffer *buff = gtk_source_buffer_new_with_language(cLang);
  // Criar source view
  GtkWidget *view = gtk_source_view_new_with_buffer(buff);
  // Adicionar source view na janela
  gtk_container_add(GTK_CONTAINER(jan), view);  

  // Conectar sinal de auto completar
  g_signal_connect (view, "show-completion", G_CALLBACK(on_areaTexto_show_completion), NULL);

  // Apresentar todas ferramentas
  gtk_widget_show_all(jan);
}

// Funcao para quando poder ser mostrada a caixa de auto completar
G_MODULE_EXPORT gboolean on_areaTexto_show_completion(GtkSourceView *view, gpointer *dadosUsuario) {
  // Obter source completion da source view
  GtkSourceCompletion *compl = gtk_source_view_get_completion(view);
  // Obter o iter da posicao atual
  GtkTextIter *pos = obterPosicaoCursor(GTK_WIDGET(view));
  if (pos == NULL)
    return FALSE;
  // Criar contexto de auto completar
  GtkSourceCompletionContext *contexto = gtk_source_completion_create_context(compl, pos);
  // Obter buffer da source view
  GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
  GList *palavras = NULL;

  // Criar novo fornecedor de palavras padrao da GTK
  GtkSourceCompletionWords *palavra = gtk_source_completion_words_new("Palavras", NULL);
  // Registrar buffer no fornecedor
  gtk_source_completion_words_register(palavra, buf);
  // Adicionar fornecedor no source completion da source view
  gtk_source_completion_add_provider(compl, GTK_SOURCE_COMPLETION_PROVIDER(palavra), NULL);
  // Adicionar fornecedor de palavras na lista
  palavras = g_list_append(palavras, GTK_SOURCE_COMPLETION_PROVIDER(palavra));

  // Mostrar janela de auto completar 
  gtk_source_completion_show(compl, palavras, contexto);
  return TRUE;
}

// Funcao para obter iter da posicao atual do cursor
GtkTextIter *obterPosicaoCursor(GtkWidget *areaTexto) {
  GtkTextIter *iter;
  GtkTextBuffer *buf;
  // Obter buffer da area de texto
  buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(areaTexto));
  // Alocado iter
  iter = (GtkTextIter *)malloc(sizeof(GtkTextIter));
  if (iter == NULL) return NULL;
  // Obter iter do inicio do buffer
  gtk_text_buffer_get_start_iter(buf, iter);
  if (iter == NULL)
    return NULL;
  do {
    // Verificar se o iter esta na posicao do cursor
    if (gtk_text_iter_is_cursor_position(iter))
      break;
    // Mover o iter em 1 caracter
    if (!gtk_text_iter_forward_char(iter))
      return NULL;
  } while (!gtk_text_iter_is_end(iter));
  return iter;
}
