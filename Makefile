#FichierMakefileracine
#Cettevariablecontientlalistedessousrépertoires
#pourlesquelsilfaudraconstruireunprogramme.
#Pourl’instant,seulleserveurwebestdanslaliste.
FOLDERS=webserver
#Indiqueàmakelesrèglesquinecorrespondentpasàlacréation
#d’unfichier
#makelanceratoujourslaconstructiondecetterèglesielleest
#demandée,mêmesiunfichier/répertoiredunomdelacibleexiste
#Onindiqueicilarègleallainsiquelesrépertoires
.PHONY: all $(FOLDERS)
#Larèglequiseraexécutéesionlancemakesansargument
all: $(FOLDERS)
#Cetterèglevalancermakedanslerépertoirewebserver
#option-Cdemake
webserver:
	make -C webserver
