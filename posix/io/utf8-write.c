/* -*- coding: utf-8-unix -*-
 * (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-02-22
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <iconv.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <itskylib.h>

static const char *TEXT = "Old Tjikko (de)\n"
  "Old Tjikko ist eine 9550 Jahre alte Gemeine Fichte (Picea abies), die in den Bergen im Nationalpark Fulufjället in der schwedischen Provinz Dalarna wächst.[1][2][3] Das Gebiet gehört zur Gemeinde Älvdalen und liegt fünf Kilometer östlich der norwegischen Grenze. Old Tjikko ist der älteste lebende individuelle Klonbaum. Es gibt aber Beispiele für noch ältere Klonkolonien, das sind mehrere Bäume, die durch ein gemeinsames Wurzelsystem verbunden sind, wie zum Beispiel Pando, dessen Wurzelsystem auf über 80.000 Jahre geschätzt wird.\n"
  "\n"
  "Old Tjikko (da)\n"
  "Old Tjikko er en 9550 år gammel rødgran, som står Fulufjället Nationalpark i Dalarna. Det er verdens ældste kendte individuelle klonede træ. Fire forskellige prøver fra trærester i jorden ved træet, udført forskere fra Umeå Universitet, blev ved hjælp af C14-datering konstateret til at være 375, 5.660, 9.000 og 9.550 år gamle. DNA viste at alle prøverne kom fra samme individ.[1]\n"
  "Træet er navngivet efter opdageren professor Leif Kullmans hund Tjikko.[2]\n"
  "\n"
  "Old Tjikko (en)\n"
  "Old Tjikko is a 9,550-year-old Norway Spruce, located on Fulufjället Mountain of Dalarna province in Sweden. Old Tjikko originally gained notoriety as the \"world\'s oldest tree\"[1] but is instead the oldest living clonal Norway Spruce - as there are older clonal trees, while no individual tree reaches Old Tjikko\'s age.\n"
  "\n"
  "Old Tjikko (es)\n"
  "El Old Tjikko es una pícea de Noruega de 9.550 años de antigüedad, ubicado en el Parque nacional de Fulufjället, Provincia de Dalarna en Suecia. Old Tjikko es el árbol clonado más antiguo del mundo. Sin embargo, hay muchos ejemplos de colonias clonales (múltiples árboles conectados por un sistema de raíces comunes) mucho más antiguas, como \"Pando\", que se estiman unos 80.000 años de edad.\n"
  "\n"
  "Old Tjikko (eo)\n"
  "Old Tjikko (esperante Malnova Tjikko) estas 9500-jara ordinara piceo, situanta en la nacia parko Fulufjället, en la provinco Dalarna. Ĝi estas unu el la plej aĝaj individuaj klonaj arboj tutmonde.\n"
  "Malkovrita en 2004 de la geologo Leif Kullman, profesoro en la Universitato de Umeå, la arbo estis nomita je la honoro de lia mortinta hundo.\n"
  "\n"
  "Old Tjikko (fr)\n"
  "Old Tjikko (vieux Tjikko) est un épicéa commun vieux de 9 550 ans, situé sur la montagne de Fulufjället en Suède, dans la province de Dalécarlie. Il s\'agit de l\'un des plus vieux arbres clonaux (arbres pouvant disposer de troncs multiples mais disposant d\'un système de racine communs) individuels au monde.\n"
  "\n"
  "विश्व का सबसे पुराना वृक्ष (hi)\n"
  "पुराना टीजिक्को विश्व के ज्ञात पेड़ों में से सबसे पुराने पेड़ों में से एक है। वैज्ञानिकों ने स्वीडन के डलारना प्रांत में दुनिया में सर्वाधिक समय तक जीवित रहने वाले यानी 9,550 वर्ष पुराने वृक्ष की खोज की है। इस पेड़ के अलावा वैज्ञानिकों ने 375 वर्ष, 5,660 वर्ष व 9,000 वर्ष पुराने वृक्षों की भी खोज की है।\n"
  "आनुवांशिकता के लिहाज से भी इन पेड़ों में काफी समानता देखने को मिली है। इन सभी वृक्षों की अमेरिका के फ्लोरिडा और मियामी शहर की प्रयोगशालाओं में जांच की गई थी। खास बात यह है कि जलवायु परिवर्तन की मार झेलने के बावजूद इन वृक्षों ने अपना अस्तित्व बचाए रखा।\n"
  "वृक्षों की खोज से जुड़े यूमिया विश्वविद्यालय के लीफ कुलमैन ने बताया, \"इस पेड़ ने हमारी पुरानी मान्यताओं को गलत साबित कर दिया है।\" यही नहीं विपरीत परिस्थितियों में भी इतने अधिक समय तक जीवित रहने वाले पेड़ों ने वैज्ञानिकों के सामने एक नया सवाल भी खड़ा कर दिया है।\n"
  "गौरतलब है कि इससे पहले उत्तरी अमेरिका में देवदार के 4,000 से 5,000 वर्ष पुराने पेड़ों के बारे में पता चला था।\n"
  "\n"
  "Old Tjikko (id)\n"
  "Dari Wikipedia bahasa Indonesia, ensiklopedia bebas\n"
  "Old Tjikko adalah pohon Norwegia Spruce berusia 9.550 tahun, yang terletak di Gunung Fulufjället provinsi Dalarna, Swedia. Old Tjikko adalah pohon tertua di dunia yang diketahui hidup klonal individu. Namun, ada banyak contoh koloni klonal jauh lebih tua (beberapa pohon yang dihubungkan dengan sistem akar yang sama), seperti \"Pando\", diperkirakan berusia lebih dari 80.000 tahun.\n"
  "\n"
  "Old Tjikko (it)\n"
  "Old Tjikko è un abete rosso di circa 9 557 anni situato su una montagna del parco nazionale Fulufjället, in Svezia. Alto 5 m, si tratta del più antico albero-clone vivente non appartenente ad un genet.[1]\n"
  "\n"
  "Old Tjikko (no)\n"
  "Old Tjikko er en gran på Fulufjället i Dalarna er verdens eldste kjente individuelle klonede tre. Fire ulike prøver fra trerester i jorden ved treet, utført forskere fra Umeå universitet, ble ved hjelp av karbondatering konstatert til å være 375, 5 660, 9 000 og 9 550 år gamle. DNA vista at alle prøvene kom fra samme individ.[1]\n"
  "\n"
  "بڈآ جکو (pnb)\n"
  "بڈآ جکو 9000 وریاں نال دنیا دا سب توں پرانا جیوندا رکھ اے. ایہ 9,550 ورے پرانا اے۔ ایہ سویڈن دے صوبے دالارنا دے پہاڑی سلسلے فولوفلالٹ وچ اے۔ مورت وچ کھڑا رکھ تے ایناں پرانا نہیں پر ایہدی جڑ پرانی اے۔\n"
  "\n"
  "Old Tjikko (pl)\n"
  "Old Tjikko − genet świerka pospolitego rosnący w Parku Narodowym Fulufjället w Szwecji. Drzewo ma 5 m wysokości. W sąsiedztwie stwierdzono pozostałości genetycznie identycznych pędów należących do tego samego osobnika znajdujące się w różnym stadium rozkładu. Najstarsze fragmenty pochodzą sprzed 9550 lat. Sukcesywnie zamierające pędy nadziemne zastępowane są u tego osobnika kolejnymi odroślami. Ocenia się, że współczesny, drzewiasty pokrój rośliny jest wynikiem ocieplenia klimatu trwającego od początków XX wieku, bowiem dawniej świerk ten rósł w formacji krzewiastej zwanej krummholz (niem. „wygięte drzewo”)[1].\n"
  "\n"
  "Старый Тикко (ru)\n"
  "Стилевые проблемы\n"
  "Стиль этой статьи неэнциклопедичен или нарушает нормы русского языка.\n"
  "Статью следует исправить согласно стилистическим правилам Википедии.\n"
  "Старый Тикко — дерево ели возрастом в 9550 лет, расположенное на горе Фулуфьеллет провинции Даларна в Швеции. Старый Тикко — старейшее из известных живущих отдельностоящих деревьев-клонов в мире. Как бы то ни было, есть множество примеров гораздо более старых клональных колоний (множеств деревьев, объединённых общей корневой системой), таких как «Пандо» возрастом, по оценкам, более 80 000 лет.\n"
  "\n"
  "Old Tjikko (sv)\n"
  "Old Tjikko är en gran på Fulufjället i Dalarna. Den är enligt tester världens äldsta kända individuella trädklon, med \"rötter\" (minst) 9 550 år tillbaka i tiden.\n";


int main(int argc, char *argv[]) {
  size_t text_len = strlen(TEXT);
  size_t buf_size = text_len * 4+7;
  char *buf_base = malloc(buf_size);

  int fd = open("old-tjikko-utf8.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  handle_error(fd, "open", PROCESS_EXIT);
  ssize_t count = write(fd, TEXT, text_len);
  handle_error(count, "write", PROCESS_EXIT);
  int retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);

  char *text = (char *) TEXT;
  char *buf  = (char *) buf_base;
  size_t in_size = text_len;
  size_t out_size = buf_size;
  iconv_t conversion = iconv_open("UTF-16BE", "UTF-8");
  size_t unconverted_size = iconv(conversion, &text, &in_size, &buf, &out_size);
  size_t converted_size = buf_size - out_size;
  printf("in_size=%ld out_size=%ld unconverted_size=%ld text_len=%ld buf_size=%ld\n", in_size, out_size, unconverted_size, text_len, buf_size);
  handle_error(unconverted_size, "iconv", PROCESS_EXIT);
  fd = open("old-tjikko-utf16be.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  handle_error(fd, "open", PROCESS_EXIT);
  count = write(fd, "\xfe\xff", 2);
  handle_error(count, "write", PROCESS_EXIT);
  count = write(fd, buf_base, converted_size);
  handle_error(count, "write", PROCESS_EXIT);
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  retcode = iconv_close(conversion);
  handle_error(retcode, "iconv_close", PROCESS_EXIT);

  text = (char *) TEXT;
  buf  = (char *) buf_base;
  in_size = text_len;
  out_size = buf_size;
  conversion = iconv_open("UTF-16LE", "UTF-8");
  unconverted_size = iconv(conversion, &text, &in_size, &buf, &out_size);
  handle_error(unconverted_size, "iconv", PROCESS_EXIT);
  fd = open("old-tjikko-utf16le.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  handle_error(fd, "open", PROCESS_EXIT);
  converted_size = buf_size - out_size;
  count = write(fd, "\xff\xfe", 2);
  handle_error(count, "write", PROCESS_EXIT);
  count = write(fd, buf_base, converted_size);
  handle_error(count, "write", PROCESS_EXIT);
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  retcode = iconv_close(conversion);
  handle_error(retcode, "iconv_close", PROCESS_EXIT);

  text = (char *) TEXT;
  buf  = (char *) buf_base;
  in_size = text_len;
  out_size = buf_size;
  conversion = iconv_open("UTF-32BE", "UTF-8");
  unconverted_size = iconv(conversion, &text, &in_size, &buf, &out_size);
  converted_size = buf_size - out_size;
  handle_error(unconverted_size, "iconv", PROCESS_EXIT);
  fd = open("old-tjikko-utf32be.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  handle_error(fd, "open", PROCESS_EXIT);
  handle_error(count, "write", PROCESS_EXIT);
  count = write(fd, buf_base, converted_size);
  handle_error(count, "write", PROCESS_EXIT);
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  retcode = iconv_close(conversion);
  handle_error(retcode, "iconv_close", PROCESS_EXIT);

  text = (char *) TEXT;
  buf  = (char *) buf_base;
  in_size = text_len;
  out_size = buf_size;
  conversion = iconv_open("UTF-32LE", "UTF-8");
  unconverted_size = iconv(conversion, &text, &in_size, &buf, &out_size);
  handle_error(unconverted_size, "iconv", PROCESS_EXIT);
  fd = open("old-tjikko-utf32le.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  handle_error(fd, "open", PROCESS_EXIT);
  converted_size = buf_size - out_size;
  count = write(fd, buf_base, converted_size);
  handle_error(count, "write", PROCESS_EXIT);
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  retcode = iconv_close(conversion);
  handle_error(retcode, "iconv_close", PROCESS_EXIT);

  text = (char *) TEXT;
  buf  = (char *) buf_base;
  in_size = text_len;
  out_size = buf_size;
  conversion = iconv_open("KOI-8//TRANSLIT", "UTF-8");
  unconverted_size = iconv(conversion, &text, &in_size, &buf, &out_size);
  handle_error(unconverted_size, "iconv", PROCESS_EXIT);
  fd = open("old-tjikko-koi8.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  handle_error(fd, "open", PROCESS_EXIT);
  converted_size = buf_size - out_size;
  count = write(fd, buf_base, converted_size);
  handle_error(count, "write", PROCESS_EXIT);
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  retcode = iconv_close(conversion);
  handle_error(retcode, "iconv_close", PROCESS_EXIT);

  text = (char *) TEXT;
  buf  = (char *) buf_base;
  in_size = text_len;
  out_size = buf_size;
  conversion = iconv_open("ISO-8859-1//TRANSLIT", "UTF-8");
  unconverted_size = iconv(conversion, &text, &in_size, &buf, &out_size);
  handle_error(unconverted_size, "iconv", PROCESS_EXIT);
  fd = open("old-tjikko-iso_8859_1.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  handle_error(fd, "open", PROCESS_EXIT);
  converted_size = buf_size - out_size;
  count = write(fd, buf_base, converted_size);
  handle_error(count, "write", PROCESS_EXIT);
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  retcode = iconv_close(conversion);
  handle_error(retcode, "iconv_close", PROCESS_EXIT);

}
