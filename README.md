
Running:

```sh
docker-compose up -d && docker-compose run app python manage.py migrate
```

Usage:

```sh
curl -F file=@/path/to/file http://localhost:8000/api/analyses
```

or

```sh
curl -d text'Tekst do analizy.' http://localhost:8000/api/analyses
```